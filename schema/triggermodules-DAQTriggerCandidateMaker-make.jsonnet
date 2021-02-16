{
    // Make a conf object for DAQTriggerCandidateMaker
    conf(window=500000000, thresh=6, hit_thresh=2) :: {
	time_window: window,
        threshold: thresh, 
	hit_threshold: hit_thresh,
    },
}
