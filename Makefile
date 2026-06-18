#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITARM)),)
$(error "Please set DEVKITARM in your environment. export DEVKITARM=<path to>devkitARM")
endif

export TOPDIR ?= $(CURDIR)
include $(DEVKITARM)/3ds_rules

TARGET		:= 	$(notdir $(CURDIR))
INCLUDES	:= 	includes

SOURCES 	:= 	src

PSF 		:= 	mk7.plgInfo

#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft

CFLAGS	:=	-mword-relocations \
 			-ffunction-sections -fdata-sections -fno-strict-aliasing \
			$(ARCH) $(BUILD_FLAGS) $(G)

CFLAGS		+=	$(INCLUDE) -D__3DS__ $(DEFINES)

#-Wall -Wextra -Wdouble-promotion -Werror

CXXFLAGS	:= $(CFLAGS) -fno-rtti -fno-exceptions -std=gnu++11

ASFLAGS		:= $(ARCH) $(G)
LDFLAGS		:= -T $(TOPDIR)/3gx.ld $(ARCH) -Os -Wl,$(WL)--gc-sections,--section-start,.text=0x07000100 #-specs=3dsx.specs

LIBS 		:=  $(BUILD_LIBS) -lm
LIBDIRS		:= 	$(CTRULIB) $(PORTLIBS)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------
export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES			:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
SFILES			:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

export LD 		:= 	$(CXX)
export OFILES	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L $(dir)/lib)

.PHONY: $(BUILD) relink all

#---------------------------------------------------------------------------------
all: $(TARGET)-release.3gx

release:
	@[ -d $@ ] || mkdir -p $@

$(TARGET)-release.3gx : release
	@$(MAKE) BUILD=release OUTPUT=$(CURDIR)/$@ BUILD_LIBS=" -lctru" WL=--strip-discarded,--strip-debug, \
	BUILD_CFLAGS="-DNDEBUG=1 -O2 -fomit-frame-pointer" DEPSDIR=$(CURDIR)/release \
	--no-print-directory -C release	-f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------

relink:
	@rm -f *.elf *.3gx
	@$(MAKE)

#---------------------------------------------------------------------------------

else

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------

DEPENDS	:=	$(OFILES:.o=.d)


$(OUTPUT) : $(basename $(OUTPUT)).elf
$(basename $(OUTPUT)).elf : $(OFILES)
#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	:	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

#---------------------------------------------------------------------------------
%.3gx: %.elf
	@echo creating $(notdir $@)
    @curl -o 3gxtool "https://gitlab.com/-/project/35893975/uploads/7cf27fcdc26921d9a6c5505c1e5bbcaa/3gxtool"
    @chmod +x ./3gxtool
    @ls
	@./3gxtool -s $^ $(TOPDIR)/$(PSF) $@

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------
