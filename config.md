# Config info

* gl - general settings for OpenGL
    * depthTest - bool, enable/disable depth test
    * msaa - bool, enable/disable msaa
    * backfaceCulling - bool, enable/disable backface culling
    * shadingModel - string, supports only "blinn" for now
* paths - paths to certain folders
    * shaderLocation - location of shaders, it is used as default location for shader loading
    * assetsLocation - location of assets for GUI/models etc...
* debug - settings for debugging purposes
    * drawMesh - bool, enable/disable rendering of terrain chunks
    * drawNormals - bool, enable/disable rendering of normals on chunks
    * drawChunkBorder - enable/disable rendering of voxels within a chunk
        * enabled - bool, enable/disable
        * step - int, defined how many cubes should be drawn within one chunk (chunk size is 32*32*32)
* render - rendering settings
    * viewDistance - float, basic view distance, affects view frustum, culling and chunk calculations based on distance to middle of a chunk
    * levelOfDetail - int, maximum amount of division within base chunk (NOT IMPLEMENTED)
    * viewFrustumCulling - bool, enable/disable view frustum culling
* marching_cubes - settings for marching cubes algorithm
    * chunkSize - float, size of chunk in space -- lower means higher resolution
    * surroundingSize - int, amount of chunks in each direction which is checked (TODO implement using endless world)
    * chunkPoolSize - int, amount of chunks which are prepared for calculations
    * aggressiveChunkUnloading - bool, if true, all chunks that are further away than 'render.viewDistance' are freed and put back into available pool, else they stay unless there are too few chunks to render new ones
    * computeBatchSize - int, maximum amount of chunks which is allowed to be calculated in one frame
* device - basic device information (TODO: currently contains only window size, which should be automated or something)
 