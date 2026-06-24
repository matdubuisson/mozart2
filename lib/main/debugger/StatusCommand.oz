local
	SchedulesCounter = {Boot_Introspection.getSchedulesCounter $}
	OperationsCounter = {Boot_Introspection.getOperationsCounter $}

	ThreadsCount = {Boot_Introspection.getThreadsCount $}
in
	{PrintInfo "Virtual machine status:"}
	
	{PrintInfo "\tSchedules counter: "#
		{Int.toString SchedulesCounter $}}
	{PrintInfo "\tOperations counter: "#
		{Int.toString OperationsCounter $}}

	{PrintInfo "\tThreads count: "#
		{Int.toString ThreadsCount $}}
end