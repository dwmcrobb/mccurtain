At the moment, just two JSON files.

The first (asn-list.json) is a list of AS numbers with their country codes ("CC") and
organization names ("Org").  It is based on a CSV list I got here on github, but I don't remember where I pulled
it from.  The CSV was very outdated and hence incorrect, and not consistent or structured.  I needed something a
bit more structured and I wanted JSON for parsing.  Note that many of the AS numbers in this file should NOT be blocked
right now, since some of the AS number were reassigned long ago (the lists I've found don't get properly updated, and I
undeerstand why... it's difficult to do programatically since RDAP is not well implemented at all registries
and WHOIS is far from consistent, and doing it manually took me an entire weekend).

The second file (asn-nets.json) is just the ASs from the first file but with their prefixes added from routeviews, getting
all prefixes that have the given AS as their origin.  Note that I coalesce these network per AS, just to make a smaller
list of prefixes if you're going to use them in a pf list.  For example, 10.1.2/24 and 10.1.3/24 would be combined to
10.1.2/23.  I also remove prefixes that are covered by a wider prefix with the same origin AS.
