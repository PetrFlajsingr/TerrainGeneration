# Config info

* gl - general settings for OpenGL
    * depthTest - bool, enable/disable depth test
    * msaa - bool, enable/disable msaa
    * backfaceCulling - bool, enable/disable backface culling
* paths - paths to certain folders
    * shaderLocation - location of shaders, it is used as default location for shader loading
    * assetsLocation - location of assets for GUI/models etc...
* render - rendering settings
    * viewDistance - float, basic view distance, affects view frustum, culling and chunk calculations based on distance to middle of a chunk
    * levelOfDetail - int, maximum amount of division within base chunk
* marching_cubes - settings for marching cubes algorithm
    * chunkSize - float, size of chunk in space -- lower means higher resolution
    * surroundingSize - int, amount of chunks in each direction which is checked
    * aggressiveChunkUnloading - bool, if true, all chunks that are further away than 'render.viewDistance' are freed and put back into available pool, else they stay unless there are too few chunks to render new ones
    * computeBatchSize - int, maximum amount of chunks which is allowed to be calculated in one frame
    * textures - paths for texture for triplanar texturing -- files must be placed within assets/textures