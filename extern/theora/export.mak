# Compile exports
export.compile.extern.theora.INCLUDE_DIRS := include
export.compile.extern.theora.IMPORTS      := compile.extern.ogg

# Link exports
export.link.extern.theora.LIBS    := funner.extern.theora
export.link.extern.theora.IMPORTS := link.extern.ogg
