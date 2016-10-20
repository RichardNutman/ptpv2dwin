README for ptpv2d

This software implements IEEE 1588 and 802.1AS Precision Timing
Protocol (PTP) and is a derivative work of the sourceforge.net
ptpd daemon software.  This software is implemented to 
support IEEE 1588 PTP version 1 and version 2 and also
IEEE 802.1AS. 

For ptpv2d, the source base used to create the this software 
was the sourceforge.net ptpd version v1rc1.  That source
was licensed as a BSD license under the following license terms:

****************************************************************

COPYRIGHT notice for PTPd

The following copyright notice applies to all files which compose the
PTPd. This notice applies as if the text was explicitly included each
file.

Copyright (c) 2005-2007 Kendall Correll

Permission is hereby granted to use, copy, modify, and distribute this software
for any purpose and without fee, provided that this notice appears in all
copies. The authors make no representations about the suitability of this
software for any purpose. This software is provided "as is" without express or
implied warranty.

**************************************************************

For ptpv2d, numerous changes have been done to the ptpd v1rc1 base to
try and make the code more clear, clean up and add comments, and also
add features with the main purpose of adding IEEE 1588 v2 support and
also IEEE 802.1AS (which uses IEEE 1588 v2 messages, algorithms and 
mechanisms).

All of the changes, modifications, enhancements, etc. done to the ptpd base 
to create ptpv2d are copyrighted by Alan K. Bartky and released under a 
GNU Public License Version 2.0. under the permission of Alan K. Bartky
and SNAP Networks Pvt. Ltd. Bangalore, Karnataka India (whom also 
retain rights of publication and licensing to the ptpv2d source code base). 

The original ptpd authors and contributors, Alan K. Bartky and SNAP
Networks Pvt. Ltd. make no representations about the suitability of this
ptpv2d software for any purpose. This software is provided "as is" without 
express or implied warranty.

For general comments or questions, you can email Alan K. Bartky at
alan@bartky.net


****************************************************************

For Ptpv2dWin, all micro-controller code has been stripped out along
with quite a bit of tidying up.
An NDIS Windows drivers will be provided at some point which will
allow timestamping at the driver level shortly after the NIC hardware
has handled the ISR from the network. Short of real hardware timestamps
this should give the best possible PTP timing under Windows.

All modifications to Alan K. Bartky's code is published here publicly
under GNU Public License Version 3.0

The code is provided as-is without any express or implied warranty.

You can contact me at richard dot nutman at gmail dot com
This code base lives at https://github.com/RichardNutman/ptpv2dwin




