#!/bin/bash
pgrep sshd > result.txt
wc -w result.txt