include ../../../../Makefile.vars

TARTARGETS  = ${STAGING_DIR}/etc/systemd/system/mccurtaind.service

all::
	echo "Nothing to be done for 'all'."

tarprep:: ${TARTARGETS}

${STAGING_DIR}/etc/systemd/system/mccurtaind.service: mccurtaind.service
	../../../../install-sh -c -m 644 $< $@

clean:: clean-tarprep

clean-tarprep::
	rm -f ${TARTARGETS}

