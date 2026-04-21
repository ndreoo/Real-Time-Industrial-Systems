# Real-Time-Industrial-Systems
The objective of this project is to carry out a Design of Experiment in order to understand the significant factors that characterize and influence a given scheduler. In particular, the Xen Hypervisor was installed on bare metal to run benchmarks in Xen domains and determine which scheduler performs best. To perform these analyses, it was necessary to enable real-time features in the Linux kernel. The schedulers evaluated are: credit2, null, and RTDS. For complete documentation, refer to the following files:

Presentation_RTIS: Project presentation and various settings.
DoE Full: which presents the process through which the Design of Experiment was carried out, along with all related statistical analyses.

Directory:

- 	# benchmark_results.zip:
    Experiments in CSV format.
		FILE NAMING NOTES: Each .csv file follows this naming scheme:
		[dom0mem]_[sched]_[stress]_[category]_[benchmark]_[iteration], where:

		dom0mem: can be 2 or 8 (memory assigned to dom0)
		sched: rtds, null, credit2
		stress: cpu, io, vm, int, net
		category: benchmark categories from the TACle suite (in our case: app, kernel, sequential, test)
		iteration: from 1 to 5 (indicates the experiment repetition).

		WARNING!!! The first round of experiments was carried out by collecting 40 samples.
		Because of this, the first two rows of each file whose name ends with “_1” must not be considered,
		since the first two values in the WakeupLatency column are affected
		by an uninitialized variable in the code.

		All other experiments were performed by collecting 42 samples and always removing
		the first two rows (they are already cleaned).
		ALL .csv files contain 40 rows of recorded values.

-	# NuoviEsp_SchedParams:
	  Experiments carried out by varying the parameters of the different schedulers to identify
		the best configuration. The different configurations were evaluated under CPU stress.
		Experiments without stress were performed only for the selected best configuration.
		benchmark_codice: Code of the “unikernelized” version of the TACle benchmark suite.
		They are organized into folders with the same name as the benchmark.
-	# DoE_FULL:
	  Complete document with the DoE carried out on all 8 benchmarks (one DoE per benchmark).
		In this case, only the default parameters for each scheduler were considered.
		The analysis of scheduler parameters was added to the file “Presentazione_RTIS”.
-	# XX_HowTo:
    Documents ending with “HowTo” are short guides related to Xen,
	  Unikraft, and PV_domU.
