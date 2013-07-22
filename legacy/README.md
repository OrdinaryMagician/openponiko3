Migration tool for OpenPONIKO 2.0 legacy text database files (only _quotes and _registry).

Legacy quotes contain no channel data, and as a result, the channel in the new format is set to "UNKNOWN".
Of course OpenPONIKO 3.0 herself deals with this properly.

Legacy registry does contain channel data, though.

It is recommended to have the destination output saved in tmpfs, for maximum speed.

Please ignore the segfault when finishing, it's harmless.