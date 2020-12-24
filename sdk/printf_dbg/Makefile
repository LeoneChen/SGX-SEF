#
# Created by leone on 2020/10/14.
#

TOP_DIR = ../..
include ../../buildenv.mk

CXXFLAGS += $(ENCLAVE_CXXFLAGS) -Werror -fno-exceptions -fno-rtti

CPPFLAGS := -I$(COMMON_DIR)/inc/internal \
            -I$(COMMON_DIR)/inc          \
            -I$(COMMON_DIR)/inc/tlibc    \
            -I$(COMMON_DIR)/../sdk/trts  \
            -I$(LINUX_PSW_DIR)

OBJ := printf_dbg.o

EDGER8R_DIR = $(LINUX_SDK_DIR)/edger8r/linux
EDGER8R = $(EDGER8R_DIR)/_build/Edger8r.native
LIBPRINTF_DBG := libsgx_printf_dbg.a

.PHONY:all
all: $(LIBPRINTF_DBG) | $(BUILD_DIR)
	$(CP) $(LIBPRINTF_DBG) $|

printf_dbg.o: sgx_printf_dbg_t.h

sgx_printf_dbg_t.h: $(COMMON_DIR)/inc/sgx_printf_dbg.edl $(EDGER8R)
	$(EDGER8R) --header-only --trusted --search-path $(TOP_DIR)/common/inc sgx_printf_dbg.edl

$(EDGER8R):
	$(MAKE) -C $(EDGER8R_DIR)

$(LIBPRINTF_DBG): $(OBJ)
	$(AR) rcs $@ $^

$(OBJ): %.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

$(BUILD_DIR):
	@$(MKDIR) $@

.PHONY: clean
clean:
	@$(RM) *.o *.a $(BUILD_DIR)/$(LIBPRINTF_DBG)
	@$(RM) sgx_printf_dbg_t.*
	$(MAKE) -C $(EDGER8R_DIR) clean

.PHONY: rebuild
rebuild:
	$(MAKE) clean
	$(MAKE) all
