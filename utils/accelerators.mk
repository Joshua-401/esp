
ACCELERATORS_PATH = $(ESP_ROOT)/accelerators
ACCELERATORS = $(filter-out common, $(shell ls -d $(ACCELERATORS_PATH)/*/ | awk -F/ '{print $$(NF-1)}'))

ACCELERATORS-clean = $(addsuffix -clean, $(ACCELERATORS))
ACCELERATORS-distclean = $(addsuffix -distclean, $(ACCELERATORS))

ACCELERATORS-sim = $(addsuffix -sim, $(ACCELERATORS))

ACCELERATORS-plot = $(addsuffix -plot, $(ACCELERATORS))

print-available-accelerators:
	$(QUIET_INFO)echo "Available accelerators: $(ACCELERATORS)"

$(ACCELERATORS):
	$(QUIET_RUN)mkdir -p $(ACCELERATORS_PATH)/$@/hls-work-$(TECHLIB)
	@cd $(ACCELERATORS_PATH)/$@/hls-work-$(TECHLIB); \
	if test ! -e project.tcl; then \
		ln -s ../stratus/project.tcl; \
	fi; \
	if test ! -e Makefile; then \
		ln -s ../stratus/Makefile; \
	fi;
	$(QUIET_MAKE)ACCELERATOR=$@ TECH=$(TECHLIB) ESP_ROOT=$(ESP_ROOT) make -C $(ACCELERATORS_PATH)/$@/hls-work-$(TECHLIB) memlib | tee $@_memgen.log
	$(QUIET_INFO)echo "Running HLS for available implementations of $@"
	$(QUIET_MAKE)ACCELERATOR=$@ TECH=$(TECHLIB) ESP_ROOT=$(ESP_ROOT) make -C $(ACCELERATORS_PATH)/$@/hls-work-$(TECHLIB) hls_all | tee $@_hls.log
	$(QUIET_INFO)echo "Installing available implementations for $@ to $(ESP_ROOT)/tech/$(TECHLIB)/acc/$@"
	$(QUIET_MAKE)ACCELERATOR=$@ TECH=$(TECHLIB) ESP_ROOT=$(ESP_ROOT) make -C $(ACCELERATORS_PATH)/$@/hls-work-$(TECHLIB) install

$(ACCELERATORS-sim):
	@$(QUIET_RUN)ACCELERATOR=$(@:-sim=) TECH=$(TECHLIB) ESP_ROOT=$(ESP_ROOT) make -C $(ACCELERATORS_PATH)/$(@:-sim=)/hls-work-$(TECHLIB) sim_all | tee $@_sim.log

$(ACCELERATORS-plot):
	@$(QUIET_RUN)ACCELERATOR=$(@:-plot=) TECH=$(TECHLIB) ESP_ROOT=$(ESP_ROOT) make -C $(ACCELERATORS_PATH)/$(@:-plot=)/hls-work-$(TECHLIB) plot

$(ACCELERATORS-clean):
	@$(QUIET_CLEAN)ACCELERATOR=$(@:-clean=) TECH=$(TECHLIB) ESP_ROOT=$(ESP_ROOT) make -C $(ACCELERATORS_PATH)/$(@:-clean=)/hls-work-$(TECHLIB) clean

$(ACCELERATORS-distclean):
	@$(QUIET_CLEAN)ACCELERATOR=$(@:-distclean=) TECH=$(TECHLIB) ESP_ROOT=$(ESP_ROOT) make -C $(ACCELERATORS_PATH)/$(@:-distclean=)/hls-work-$(TECHLIB) distclean
	@$(RM) $(ESP_ROOT)/tech/$(TECHLIB)/acc/$(@:-distclean=)

.PHONY: print-available-accelerators $(ACCELERATORS) $(ACCELERATORS-sim) $(ACCELERATORS-plot) $(ACCELERATORS-clean) $(ACCELERATORS-distclean)
