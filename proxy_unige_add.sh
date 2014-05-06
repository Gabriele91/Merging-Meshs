#!/usr/bin/env bash
export http_proxy=http://wifiproxy.unige.it:80/
export https_proxy=http://wifiproxy.unige.it:80/
git add "$@"
