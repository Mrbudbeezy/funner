#section ClientToServer

void LoadResource (const char* name);
void UnloadResource (const char* name);

void SetMaxDrawDepth (uint32 depth);

void CreateTexture (const char* texture_name, media::Image image, TextureDimension dimension, bool8 create_mipmaps);
void UpdateTexture (const char* texture_name, media::Image image);
void RemoveTexture (const char* texture_name);

void CloneMaterial (const char* material_name, const char* prototype_name);
void SetMaterialTexmapImage (const char* material_name, const char* semantic, const char* image_name);
void SetMaterialTexmapSampler (const char* material_name, const char* semantic, const char* sampler);
void RemoveMaterial (const char* material_name);

void SetViewportArea (object_id_t id, int32 left, int32 top, int32 width, int32 height);
void SetViewportZOrder (object_id_t id, int32 zorder);
void SetViewportActive (object_id_t id, bool8 is_active);
void SetViewportName (object_id_t id, const char* name);
void SetViewportTechnique (object_id_t id, const char* name);
void SetViewportBackground (object_id_t id, bool8 state, const math::vec4f& color);
void SetViewportProperties (object_id_t id, uint64 properties_id);
void SetViewportScene (object_id_t id, object_id_t scene_id);
void SetViewportCameraWorldMatrix (object_id_t id, const math::mat4f& tm);
void SetViewportCameraProjectionMatrix (object_id_t id, const math::mat4f& tm);
void SetViewportCameraName (object_id_t id, const char* name);

void CreateRenderTarget (object_id_t id, const char* name, const char* init_string);
void DestroyRenderTarget (object_id_t id);
void SetRenderTargetActive (object_id_t id, bool8 active);
void SetRenderTargetScreenArea (object_id_t id, int32 left, int32 top, int32 width, int32 height);
void SetRenderTargetBackground (object_id_t id, bool8 state, const math::vec4f& color);
void AttachViewportToRenderTarget (object_id_t id, object_id_t viewport_id);
void DetachViewportFromRenderTarget (object_id_t id, object_id_t viewport_id);
void UpdateRenderTarget (object_id_t id);

void UpdatePropertyMap (...);
void RemovePropertyMap (object_id_t id);
void RemovePropertyLayout (object_id_t id);

void CreateScene (object_id_t id);
void DestroyScene (object_id_t id);
void SetSceneName (object_id_t id, const char* name);
void SetSceneNodes (object_id_t id, RawArray<object_id_t> nodes);

void CreateNode (object_id_t id, NodeType type);
void DestroyNode (object_id_t id);
void SetNodeName (object_id_t id, const char* name);
void SetNodeWorldMatrix (object_id_t id, const math::mat4f& tm);
void SetNodeScene (object_id_t id, object_id_t scene_id);

void SetEntityBounds (object_id_t id, bool is_infinite, const bound_volumes::aaboxf& box);
void SetEntityVisibility (object_id_t id, bool state);

void SetVisualModelScissor (object_id_t id, object_id_t scissor_id);
void SetVisualModelDynamicShaderProperties (object_id_t id, object_id_t properties_id);
void SetVisualModelStaticShaderProperties (object_id_t id, object_id_t properties_id);

void SetStaticMeshName (object_id_t id, const char* mesh_name);

void SetLightParams (object_id_t id, const LightParams& params);

void SetPageCurlParams (object_id_t id, const PageCurlParams& params, const char* front_left_material, const char* front_right_material, const char* back_left_material, const char* back_right_material);

void SetSpriteListParams (object_id_t id, SpriteMode mode, PrimitiveUsage usage, const math::vec3f& up, const char* batch);
void SetSpriteListMaterial (object_id_t id, const char* material);
void SetSpriteListBuffer (object_id_t id, uint32 count, uint32 reserve_count);
void SetSpriteListDescs (object_id_t id, uint32 first, RawArray<SpriteDesc> descs);

void SetLineListParams (object_id_t id, PrimitiveUsage usage, const char* batch);
void SetLineListMaterial (object_id_t id, const char* material);
void SetLineListBuffer (object_id_t id, uint32 count, uint32 reserve_count);
void SetLineListDescs (object_id_t id, uint32 first, RawArray<LineDesc> descs);

#section ServerToClient

void UpdatePropertyMap (...);
void RemovePropertyMap (object_id_t id);
void RemovePropertyLayout (object_id_t id);
