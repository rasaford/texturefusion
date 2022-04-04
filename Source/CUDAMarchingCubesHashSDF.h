#pragma once

#include "GlobalAppState.h"
#include "VoxelUtilHashSDF.h"
#include "MarchingCubesSDFUtil.h"
#include "CUDASceneRepChunkGrid.h"

#include <opencv2\core.hpp>

class CUDAMarchingCubesHashSDF
{
public:
	CUDAMarchingCubesHashSDF(const MarchingCubesParams& params) {
		create(params);
	}

	~CUDAMarchingCubesHashSDF(void) {
		destroy();
	}

	static MarchingCubesParams parametersFromGlobalAppState(const GlobalAppState& gas) {
		MarchingCubesParams params;
		params.m_maxNumTriangles = gas.s_marchingCubesMaxNumTriangles;
		params.m_threshMarchingCubes = gas.s_SDFMarchingCubeThreshFactor*gas.s_SDFVoxelSize;
		params.m_threshMarchingCubes2 = gas.s_SDFMarchingCubeThreshFactor*gas.s_SDFVoxelSize;
		params.m_sdfBlockSize = SDF_BLOCK_SIZE;
		params.m_hashBucketSize = HASH_BUCKET_SIZE;
		params.m_hashNumBuckets = gas.s_hashNumBuckets;
		params.m_texGlobalWidth = gas.s_texGlobalWidth;
		params.m_texPoolPatchWidth = gas.s_texPoolPatchWidth;
		params.m_texPatchPadding = gas.s_texPatchPadding;
		return params;
	}
	
	
	void reset(void) {
		m_meshData.clear();
		m_meshOnlyData.clear();
		globalTexMap->setTo(cv::Scalar(0, 0, 0));
	}

	//! copies the intermediate result of extract isoSurfaceCUDA to the CPU and merges it with meshData
	//void copyTrianglesToCPU(const TexPoolData& texPoolData, const TexPoolParams& texPoolParams);
	void saveMesh(const std::string& filename, const mat4f *transform = NULL, bool overwriteExistingFile = false);


	//void extractIsoSurface(CUDASceneRepChunkGrid& chunkGrid, const RayCastData& rayCastData, const vec3f& camPos, float radius);
	void extractIsoSurface(CUDASceneRepChunkGrid & chunkGrid, const RayCastData & rayCastData, const TexPoolData & texPoolData, const TexPoolParams & texPoolParams, const vec3f & camPos, float radius);
	void extractIsoSurface(const HashData & hashData, const HashParams & hashParams, const RayCastData & rayCastData, const TexPoolData & texPoolData, const TexPoolParams & texPoolParams, 
		const vec3f& minCorner, const vec3f& maxCorner, bool boxEnabled);
	//void extractIsoSurface(const HashData& hashData, const HashParams& hashParams, const RayCastData& rayCastData, const vec3f& minCorner = vec3f(0.0f, 0.0f, 0.0f), const vec3f& maxCorner = vec3f(0.0f, 0.0f, 0.0f), bool boxEnabled = false);

	//void extractIsoSurfaceCPU(const HashData& hashData, const HashParams& hashParams, const RayCastData& rayCastData);

	


private:
	
	void create(const MarchingCubesParams& params);
	void destroy(void);

	void copyTrianglesToCPU(TexPoolData texPoolData, TexPoolParams texPoolParams);
	
	// Adds a texture patch in local coordinates to the global texture
	// @returns global (u, v) texture coordinates
	vec2f addToGlobalTexture(const uint patchTexIdx, const cv::Mat &texPatch);

	std::unique_ptr<cv::Mat> globalTexMap;
	vec2i freeTexPatchCoords;
	uint globalPatchIdx;
	uint blockPatchStartIdx;
	std::unique_ptr<std::vector<uint>> blockLocalIdxs;
	// uint globalTexMapWidth;
	// uint numGlobalTexTilesWidth;

	MarchingCubesParams m_params;
	MarchingCubesData	m_data;

	MeshDataf m_meshData;
	MeshDataf m_meshOnlyData;

	Timer m_timer;
};

