#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

TARGET		:= $(notdir $(CURDIR))
INCLUDES	:= include
SOURCES		:= src src/ctrulibExtension
PSF			:= mk7.plgInfo
3GXTOOL		:= $(CURDIR)/3gxtool
3GXTOOL_URL	:= https://gitlab.com/-/project/35893975/uploads/7cf27fcdc26921d9a6c5505c1e5bbcaa/3gxtool

#---------------------------------------------------------------------------------
# Code generation options
#---------------------------------------------------------------------------------
ARCH		:= -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS		:= -mword-relocations \
			-ffunction-sections -fdata-sections -fno-strict-aliasing \
			$(ARCH) $(BUILD_FLAGS) $(G) $(INCLUDE) -D__3DS__ $(DEFINES)

ASFLAGS		:= $(ARCH) $(G)
LDFLAGS		:= -T $(TOPDIR)/3gx.ld $(ARCH) -Os -Wl,$(WL)--gc-sections,--section-start,.text=0x07000100

LIBS		:= $(BUILD_LIBS) -lm
LIBDIRS		:= $(CTRULIB) $(PORTLIBS)

#---------------------------------------------------------------------------------
# Build rules
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))

export VPATH	:= $(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
				$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:= $(CURDIR)/$(BUILD)

CFILES		:= $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
SFILES		:= $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export LD		:= $(CXX)
export OFILES	:= $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:= $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
				$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
				-I$(CURDIR)/$(BUILD)

export LIBPATHS	:= $(foreach dir,$(LIBDIRS),-L $(dir)/lib)

.PHONY: $(BUILD) all $(TARGET)-release.3gx $(TARGET)-debug.3gx

#---------------------------------------------------------------------------------
# Main targets
#---------------------------------------------------------------------------------
all: $(TARGET)-release.3gx $(TARGET)-debug.3gx

$(TARGET)-release.3gx :
    @[ -d release] || mkdir -p release
	@$(MAKE) BUILD=release OUTPUT=$(CURDIR)/$@ BUILD_LIBS=" -lctru" WL=--strip-discarded,--strip-debug, \
	BUILD_CFLAGS="-DNDEBUG=1 -O2 -fomit-frame-pointer" DEPSDIR=$(CURDIR)/release \
	--no-print-directory -C release -f $(CURDIR)/Makefile

$(TARGET)-debug.3gx :
    @[ -d debug] || mkdir -p debug
	@$(MAKE) BUILD=debug OUTPUT=$(CURDIR)/$@ BUILD_LIBS="-lctrpfd -lctrud" BUILD_CFLAGS="-DDEBUG=1 -Og" G=-g \
	DEPSDIR=$(CURDIR)/debug --no-print-directory -C debug -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
else

DEPENDS	:= $(OFILES:.o=.d)

$(OUTPUT) : $(basename $(OUTPUT)).elf
$(basename $(OUTPUT)).elf : $(OFILES)

#---------------------------------------------------------------------------------
# Binary data rules
#---------------------------------------------------------------------------------
%.bin.o : %.bin
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
# 3GX build rule
#---------------------------------------------------------------------------------
$(3GXTOOL):
	@echo "Downloading 3gxtool..."
	@curl -L -o $@ "$(3GXTOOL_URL)"
	@chmod +x $@

%.3gx : %.elf $(3GXTOOL)
	@echo creating $(notdir $@)
	@$(3GXTOOL) -s $< $(TOPDIR)/$(PSF) $@

-include $(DEPENDS)

endif
#---------------------------------------------------------------------------------
