local
	SchedulesCounter = {Boot_Introspection.getSchedulesCounter $}
	OperationsCounter = {Boot_Introspection.getOperationsCounter $}
	SystemSchedulesCounter = {Boot_Introspection.getSystemSchedulesCounter $}
	SystemOperationsCounter = {Boot_Introspection.getSystemOperationsCounter $}

	ThreadsCount = {Boot_Introspection.getThreadsCount $}
in
	{PrintInfo "Virtual machine status:"}
	
	{PrintInfo "\tSchedules counter: "#
		{Int.toString SchedulesCounter $}}
	{PrintInfo "\tOperations counter: "#
		{Int.toString OperationsCounter $}}

	{PrintInfo "\tSystem schedules counter: "#
		{Int.toString SystemSchedulesCounter $}}
	{PrintInfo "\tSystem operations counter: "#
		{Int.toString SystemOperationsCounter $}}

	{PrintInfo "\tNon-system schedules counter: "#
		{Int.toString (SchedulesCounter - SystemSchedulesCounter) $}}
	{PrintInfo "\tNon-system operations counter: "#
		{Int.toString (OperationsCounter - SystemOperationsCounter) $}}

	{PrintInfo "\tThreads count: "#
		{Int.toString ThreadsCount $}}
end