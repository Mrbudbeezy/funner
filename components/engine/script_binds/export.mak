# Link script_binds exports
export.link.engine.script_binds.LIBS    := funner.engine.script_binds
export.link.engine.script_binds.IMPORTS := link.script.core
export.link.engine.script_binds.MULTI_IMPORTS := 1

# Link bv exports
export.link.engine.script_binds.bv.LINK_INCLUDES := BoundVolumesScriptBind
export.link.engine.script_binds.bv.IMPORTS       := link.engine.script_binds

# Link common exports
export.link.engine.script_binds.common.LINK_INCLUDES := CommonScriptBind
export.link.engine.script_binds.common.IMPORTS       := link.engine.script_binds link.common.xml

# Link engine exports
export.link.engine.script_binds.engine.LINK_INCLUDES := EngineScriptBind
export.link.engine.script_binds.engine.IMPORTS       := link.engine.script_binds link.engine.core link.scene_graph.core \
                                                        link.input.manager

# Link input exports
export.link.engine.script_binds.input.LINK_INCLUDES := InputScriptBind
export.link.engine.script_binds.input.IMPORTS       := link.engine.script_binds link.input.manager

# Link lua override exports
export.link.engine.script_binds.lua_override.LINK_INCLUDES := LuaOverrideScriptBind
export.link.engine.script_binds.lua_override.IMPORTS       := link.engine.script_binds

# Link math exports
export.link.engine.script_binds.math.LINK_INCLUDES := MathScriptBind
export.link.engine.script_binds.math.IMPORTS       := link.engine.script_binds

# Link media players exports
export.link.engine.script_binds.media.players.LINK_INCLUDES := MediaPlayersScriptBind
export.link.engine.script_binds.media.players.IMPORTS       := link.engine.script_binds link.media.players.core

# Link rms exports
export.link.engine.script_binds.rms.LINK_INCLUDES := RmsScriptBind
export.link.engine.script_binds.rms.IMPORTS       := link.engine.script_binds link.media.rms

# Link sg exports
export.link.engine.script_binds.sg.LINK_INCLUDES := SceneGraphScriptBind
export.link.engine.script_binds.sg.IMPORTS       := link.engine.script_binds link.scene_graph.core

# Link system exports
export.link.engine.script_binds.system.LINK_INCLUDES := SystemScriptBind
export.link.engine.script_binds.system.IMPORTS       := link.engine.script_binds link.system
