# watchscsi

Watch the status of a SCSI device and turn it online when it goes offline

This is a tool written for myself while fighting with a faulty LSI Logic / Symbios Logic MegaRAID SAS 2208 RAID controller:

```
Apr  2 12:42:09 wildwolf kernel: [656183.160235] megasas: Found FW in FAULT state, will reset adapter.
Apr  2 12:42:09 wildwolf kernel: [656183.160237] megaraid_sas: resetting fusion adapter.
Apr  2 12:43:56 wildwolf kernel: [656290.363894] megaraid_sas: Diag reset adapter never cleared!
Apr  2 12:45:43 wildwolf kernel: [656397.567552] megaraid_sas: Diag reset adapter never cleared!
Apr  2 12:47:31 wildwolf kernel: [656504.775214] megaraid_sas: Diag reset adapter never cleared!
Apr  2 12:47:31 wildwolf kernel: [656504.775216] megaraid_sas: Reset failed, killing adapter.
Apr  2 12:47:31 wildwolf kernel: [656504.775220] sd 0:2:0:0: Device offlined - not ready after error recovery
...
Apr  2 12:47:31 wildwolf kernel: [656504.775353] sd 0:2:0:0: rejecting I/O to offline device
Apr  2 12:47:31 wildwolf kernel: [656504.786146] sd 0:2:0:0: [sda] killing request
```

In this case the error recovery sets the SCSI device offline, the file system becomes read only, and the server finally crashes.

However, it might be possible to turn the device back online by writing `running` to `/sys/bus/scsi/devices/<scsi_host_no>:0:<scsi_id>:<scsi_lun>/state`.

This program monitors the state of the device, when the state is `offline`, it tries to change it to `running`.

Usage:

watchscsi deviceid

For example: `watchscsi 0:2:0:0`

In this case the program will monitor `/sys/bus/scsi/devices/0:2:0:0/state`

`usr.local.bin.watchscsi.SAMPLE` is a *sample* Apprmor profile for this program.
