Directory:

- benchmark_results.zip : Esperimenti in formato csv. 
	NOTE SULLA NOMENCLATURA DEI FILE: Ogni file .csv ha un nome che rispetta il seguente schema:
	[dom0mem]_[sched]_[stress]_[category]_[benchmark]_[iteration], dove:
		- dom0mem : può valere 2 o 8 (memoria assegnata al dom0)
		- sched : rtds, null, credit2
		- stress : cpu, io, vm, int, net
		- category: categorie di benchmark della suite TACle (nel nostro caso: app, kernel, sequential, test)
		- iteration : da 1 a 5 (indica la ripetizione dell'esperimento).
  	ATTENZIONE !!! Il primo giro di esperimenti è stato effettuato raccogliendo 40 campioni,
	a causa di ciò non bisogna considerare le prime due righe di ogni file il cui nome termina con "_1",
	poiché i primi due valori relativi alla colonna WakeupLatency sono influenzati
	da una non inizializzazione della relativa variabile nel codice.
	Tutti gli altri esperimenti sono stati eseguiti raccogliendo 42 campioni ed eliminando
	sempre le prime due righe (sono già ripuliti).
	TUTTI i file .csv hanno 40 righe di valori catturati.

- NuoviEsp_SchedParams : Esperimenti effettuati variando i parametri dei vari scheduler per identificare
	la configurazione migliore. Le diverse configurazioni sono state valutate sotto stress-cpu.
	Gli esperimenti senza stress sono stati effettuati solo per la configurazione migliore scelta.

- benchmark_codice : Codici della versione 'unikernellizata' dei benchmark della suite TACle benchmarks.
	Sono organizzati in cartelle omonime (al benchamrk).

- DoE_FULL : Documento completo con il DoE effettuato su tutti gli 8 benchmark (1 DoE per ogni benchamrk). 
	In questo caso sono stati considerati solo i parametri di default per ogni schedueler.
	L'analisi dei parametri degli scheduler è stata aggiunta al file 'Presentazione_RTIS'.

- XX_HowTo : I documenti che terminano con 'HowTo' sono delle mini-guide riguardanti Xen, 
	Unikraft e PV_domU. 
