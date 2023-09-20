.PHONY : all

#n.b. the '-y' sets autoexec scripts to 'on' so that driver expressions will work
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	BLENDER = /Applications/Blender/blender.app/Contents/MacOS/blender -y
else ifeq ($(UNAME_S),Linux)
	BLENDER = ../../blender-2.93.4-linux-x64/blender -y
else
	BLENDER = blender
endif
BLENDER = Blender

EXPORT_MESHES=export-meshes.py
EXPORT_SCENE=export-scene.py

DIST=../dist

all : \
	$(DIST)/world.pnct \
	$(DIST)/world.scene \


$(DIST)/world.scene : world.blend $(EXPORT_SCENE)
	$(BLENDER) --background --python $(EXPORT_SCENE) -- '$<':Main '$@'

$(DIST)/world.pnct : world.blend $(EXPORT_MESHES)
	$(BLENDER) --background --python $(EXPORT_MESHES) -- '$<':Main '$@'
