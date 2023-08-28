Slicer
======

This prototype allows for time-domain RAN slicing on the downlink based on the
proportional allocation of LTE subframes to each slice. Slices are currently
static, and defined using a configuration file (see
srsenb/slice_db.csv.example), which includes the slice names, the proportion of
downlink subframes they will receive, and the international mobile subscriber
identities (IMSIs) bound to each slice.

## Usage Instructions

1. Define your slices at `/etc/srslte/slice_db.csv` according to the conventions
   outlined in the example file.
2. Start (or restart) srsepc.
3. Start srsenb with a flag to enable the slicer: `srsenb --slicer.enable=true`.

## Notes

The eNB doesn't typically maintain any relationship between IMSIs and the radio
network temporary identifiers (RNTIs) used to identify UEs in the RAN, so the
slicer must harvest and track the IMSI-to-RNTI mapping for each UE by decoding
the NAS PDUs that carry the IMSI. This currently has the following impacts on
the slicer:

1. It is necessary to restart srsepc each time you want to run srsenb with the
   slicer enabled. This guarantees an IMSI will be sent by the UE during attach,
   allowing the slicer to associate its RNTI to the appropriate slice.
2. A UE will lose its slice if it enters RRC Idle mode. This will be
   fixed in a future update, but setting a very large rrc\_inactivity\_timer for
   srsenb will keep this from happening in the meantime.

The slicer currently implements a "worksharing" model, in which UEs from the
slice that owns the current subframe are scheduled first, followed by UEs from
other slices if there are remaining resource blocks available in the subframe.
If there are UEs listed in the user\_db.csv file used by srsepc that don't
belong to any of the slices defined in the slice\_db.csv file, they will also
be scheduled after the UEs associated with the current slice.
