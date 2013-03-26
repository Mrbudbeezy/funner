# Compile exports
export.compile.extern.devil.INCLUDE_DIRS            := include
export.compile.extern.devil.no_dll.COMPILER_DEFINES := IL_STATIC_LIB
export.compile.extern.devil.unistd.COMPILER_DEFINES := HAVE_UNISTD_H

# Link exports
export.link.extern.devil.LIBS    := funner.extern.devil funner.extern.ilu
export.link.extern.devil.IMPORTS := link.extern.jpeg link.extern.tiff link.extern.libpng link.extern.zlib

# Run exports
ifeq (,$(filter no_dll,$(PROFILES)))
export.run.extern.devil.DLLS := funner.extern.devil funner.extern.ilu
else
export.run.extern.devil.DLLS :=
endif

