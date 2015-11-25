#include "culling_system.h"
#include "lumix.h"

#include "core/array.h"
#include "core/binary_array.h"
#include "core/free_list.h"
#include "core/frustum.h"
#include "core/sphere.h"

#include "core/mtjd/group.h"
#include "core/mtjd/manager.h"
#include "core/mtjd/job.h"

namespace Lumix
{
typedef BinaryArray VisibilityFlags;
typedef Array<int64> LayerMasks;

static const int MIN_ENTITIES_PER_THREAD = 50;

static void doCulling(int start_index,
	const Sphere* LUMIX_RESTRICT start,
	const Sphere* LUMIX_RESTRICT end,
	const VisibilityFlags& visiblity_flags,
	const Frustum* LUMIX_RESTRICT frustum,
	const int64* LUMIX_RESTRICT layer_masks,
	int64 layer_mask,
	CullingSystem::Subresults& results)
{
	int i = start_index;
	ASSERT(results.empty());
	for (const Sphere *sphere = start; sphere <= end; sphere++, ++i)
	{
		if (frustum->isSphereInside(sphere->m_position, sphere->m_radius) && visiblity_flags[i] &&
			((layer_masks[i] & layer_mask) != 0))
		{
			results.push(i);
		}
	}
}

class CullingJob : public MTJD::Job
{
public:
	CullingJob(const CullingSystem::InputSpheres& spheres,
		const VisibilityFlags& visibility_flags,
		const LayerMasks& layer_masks,
		int64 layer_mask,
		CullingSystem::Subresults& results,
		int start,
		int end,
		const Frustum& frustum,
		MTJD::Manager& manager,
		IAllocator& allocator,
		IAllocator& job_allocator)
		: Job(Job::AUTO_DESTROY, MTJD::Priority::Default, manager, allocator, job_allocator)
		, m_spheres(spheres)
		, m_results(results)
		, m_start(start)
		, m_end(end)
		, m_frustum(frustum)
		, m_visibility_flags(visibility_flags)
		, m_layer_masks(layer_masks)
		, m_layer_mask(layer_mask)
	{
		setJobName("CullingJob");
		m_results.reserve(end - start);
		ASSERT(m_results.empty());
		m_is_executed = false;
	}

	~CullingJob() {}

	void execute() override
	{
		ASSERT(m_results.empty() && !m_is_executed);
		doCulling(m_start,
			&m_spheres[m_start],
			&m_spheres[m_end],
			m_visibility_flags,
			&m_frustum,
			&m_layer_masks[0],
			m_layer_mask,
			m_results);
		m_is_executed = true;
	}

private:
	const CullingSystem::InputSpheres& m_spheres;
	CullingSystem::Subresults& m_results;
	const VisibilityFlags& m_visibility_flags;
	const LayerMasks& m_layer_masks;
	int64 m_layer_mask;
	int m_start;
	int m_end;
	const Frustum& m_frustum;
	bool m_is_executed;
};

class CullingSystemImpl : public CullingSystem
{
public:
	CullingSystemImpl(MTJD::Manager& mtjd_manager, IAllocator& allocator)
		: m_allocator(allocator)
		, m_job_allocator(allocator)
		, m_visibility_flags(allocator)
		, m_spheres(allocator)
		, m_result(allocator)
		, m_sync_point(true, allocator)
		, m_mtjd_manager(mtjd_manager)
		, m_layer_masks(m_allocator)
	{
		m_result.emplace(m_allocator);
		int cpu_count = (int)m_mtjd_manager.getCpuThreadsCount();
		while (m_result.size() < cpu_count)
		{
			m_result.emplace(m_allocator);
		}
	}


	~CullingSystemImpl() {}


	void clear() override
	{
		m_spheres.clear();
		m_visibility_flags.clear();
		m_layer_masks.clear();
	}


	IAllocator& getAllocator() { return m_allocator; }


	const Results& getResult() override
	{
		if (m_is_async_result)
		{
			m_sync_point.sync();
		}
		return m_result;
	}


	void cullToFrustum(const Frustum& frustum, int64 layer_mask) override
	{
		for (int i = 0; i < m_result.size(); ++i)
		{
			m_result[i].clear();
		}
		if (!m_spheres.empty())
		{
			doCulling(0,
				&m_spheres[0],
				&m_spheres.back(),
				m_visibility_flags,
				&frustum,
				&m_layer_masks[0],
				layer_mask,
				m_result[0]);
		}
		m_is_async_result = false;
	}


	void cullToFrustumAsync(const Frustum& frustum, int64 layer_mask) override
	{
		int count = m_spheres.size();

		if (count == 0) return;

		if (count < m_result.size() * MIN_ENTITIES_PER_THREAD)
		{
			cullToFrustum(frustum, layer_mask);
			return;
		}
		m_is_async_result = true;

		int cpu_count = m_mtjd_manager.getCpuThreadsCount();
		int step = count / cpu_count;
		int i = 0;
		CullingJob* jobs[16];
		ASSERT(lengthOf(jobs) >= cpu_count);
		for (; i < cpu_count - 1; i++)
		{
			m_result[i].clear();
			CullingJob* cj = LUMIX_NEW(m_job_allocator, CullingJob)(m_spheres,
				m_visibility_flags,
				m_layer_masks,
				layer_mask,
				m_result[i],
				i * step,
				(i + 1) * step - 1,
				frustum,
				m_mtjd_manager,
				m_allocator,
				m_job_allocator);
			cj->addDependency(&m_sync_point);
			jobs[i] = cj;
		}

		m_result[i].clear();
		CullingJob* cj = LUMIX_NEW(m_job_allocator, CullingJob)(m_spheres,
			m_visibility_flags,
			m_layer_masks,
			layer_mask,
			m_result[i],
			i * step,
			count - 1,
			frustum,
			m_mtjd_manager,
			m_allocator,
			m_job_allocator);
		cj->addDependency(&m_sync_point);
		jobs[i] = cj;

		for (i = 0; i < cpu_count; ++i)
		{
			m_mtjd_manager.schedule(jobs[i]);
		}
	}


	void setLayerMask(int index, int64 layer) override { m_layer_masks[index] = layer; }


	int64 getLayerMask(int index) override { return m_layer_masks[index]; }


	void enableStatic(int index) override { m_visibility_flags[index] = true; }


	void disableStatic(int index) override { m_visibility_flags[index] = false; }


	void addStatic(const Sphere& sphere) override
	{
		m_spheres.push(sphere);
		m_visibility_flags.push(true);
		m_layer_masks.push(1);
	}


	void removeStatic(int index) override
	{
		ASSERT(index <= m_spheres.size());

		m_spheres.erase(index);
		m_visibility_flags.erase(index);
		m_layer_masks.erase(index);
	}


	void updateBoundingRadius(float radius, int index) override
	{
		m_spheres[index].m_radius = radius;
	}


	void updateBoundingPosition(const Vec3& position, int index) override
	{
		m_spheres[index].m_position = position;
	}


	void insert(const InputSpheres& spheres) override
	{
		for (int i = 0; i < spheres.size(); i++)
		{
			m_spheres.push(spheres[i]);
			m_visibility_flags.push(true);
			m_layer_masks.push(1);
		}
	}


	const InputSpheres& getSpheres() override { return m_spheres; }


private:
	IAllocator& m_allocator;
	FreeList<CullingJob, 8> m_job_allocator;
	VisibilityFlags m_visibility_flags;
	InputSpheres m_spheres;
	Results m_result;
	LayerMasks m_layer_masks;

	MTJD::Manager& m_mtjd_manager;
	MTJD::Group m_sync_point;
	bool m_is_async_result;
};


CullingSystem* CullingSystem::create(MTJD::Manager& mtjd_manager, IAllocator& allocator)
{
	return LUMIX_NEW(allocator, CullingSystemImpl)(mtjd_manager, allocator);
}


void CullingSystem::destroy(CullingSystem& culling_system)
{
	LUMIX_DELETE(static_cast<CullingSystemImpl&>(culling_system).getAllocator(), &culling_system);
}
}