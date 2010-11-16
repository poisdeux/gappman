#!/bin/bash
GMNETMAND="./gm_netmand"
TEST_FAILED=0
retval=""

if [ ! -x /usr/bin/dbus-send ]
then
	echo "ERROR: /usr/bin/dbus-send not found."
	exit 1
fi

if [ ! -x ${GMNETMAND} ]
then
	echo "ERROR: ${GMNETMAND} not found"
	exit 1
fi

echo "Starting ${GMNETMAND}"
${GMNETMAND} > /dev/null 2>&1 &
retval=$?
GMNETMAND_PID=$!

if [ ${retval} -ne 0 ]
then
	echo "ERROR: ${GMNETMAND} failed"
	exit 1
fi

echo "Testing gappman.netman.NetmanInterface.RunCommand"

# Test: correct command should return exit-code 0
retval="$(dbus-send --print-reply --type=method_call --dest=gappman.netman \
  /GmNetmand gappman.netman.NetmanInterface.RunCommand \
  string:"echo" array:string:"hello"," ","world" 2> /dev/null \
  | grep ' [0-9][0-9]*' | sed 's/.* \([0-9][0-9]*\)/\1/')"

if [ "${retval}" == "0" ]
then	
  echo "Test: correct command: PASSED"
else
  echo "Test: correct command: FAILED"
  TEST_FAILED=1
fi

# Test: non-existing command should return exit-code != 0
retval="$(dbus-send --print-reply --type=method_call --dest=gappman.netman \
  /GmNetmand gappman.netman.NetmanInterface.RunCommand \
  string:"badc0mmand" array:string:"hello" 2> /dev/null \
  | grep ' [0-9][0-9]*' | sed 's/.* \([0-9][0-9]*\)/\1/')"

if [ "${retval}" == "0" ]
then
  echo "Test: nonexisting command should fail: FAILED"
  TEST_FAILED=1
else
  echo "Test: nonexisting command should fail: PASSED"
fi

# Test: command with wrong arguments should return exit-code != 0
retval="$(dbus-send --print-reply --type=method_call --dest=gappman.netman \
  /GmNetmand gappman.netman.NetmanInterface.RunCommand \
  string:"mv" array:string:"blah" 2> /dev/null \
  | grep ' [0-9][0-9]*' | sed 's/.* \([0-9][0-9]*\)/\1/')"

if [ "${retval}" == "0" ]
then
  echo "Test: command with wrong arguments should fail: FAILED"
  TEST_FAILED=1
else
  echo "Test: command with wrong arguments should fail: PASSED"
fi

# Test: command with no arguments should return exit-code 0
retval="$(dbus-send --print-reply --type=method_call --dest=gappman.netman \
  /GmNetmand gappman.netman.NetmanInterface.RunCommand \
  string:"echo" array:string:"" 2> /dev/null \
  | grep ' [0-9][0-9]*' | sed 's/.* \([0-9][0-9]*\)/\1/')"

if [ "${retval}" == "0" ]
then
  echo "Test: command without arguments should pass: PASSED"
else
  echo "Test: command without arguments shoudl pass: FAILED"
  TEST_FAILED=1
fi

# Test: malformed dbus call should fail
dbus-send --print-reply --type=method_call --dest=gappman.netman \
  /GmNetmand gappman.netman.NetmanInterface.RunCommand \
  string:"echo" > /dev/null 2>&1

if [ $? -eq 0 ]
then
  echo "Test: malformed dbus call should fail: FAILED"
  TEST_FAILED=1
else
  echo "Test: malformed dbus call should fail: PASSED"
fi

echo "Stopping gmnetmand"
kill $GMNETMAND_PID

exit ${TEST_FAILED}
