

# Introduction #

NetMan provides simple network status and restart functionality.

Using multiple checks it will provide status information about the network.
Besides checking the status it can restart the network configuration.

It consists of an applet which must be loaded by gappman. The applet
provides a user-interface to the user showing status
information through the button image. It can also be used to analyze network
problems and restart a network interface.

# Screenshot #

![http://wiki.gappman.googlecode.com/git/NetMan-status.jpg](http://wiki.gappman.googlecode.com/git/NetMan-status.jpg)

![http://wiki.gappman.googlecode.com/git/NetMan-info.jpg](http://wiki.gappman.googlecode.com/git/NetMan-info.jpg)

# Configuration #

```
<netman>
  <stati>
    <logounavail>./applets/netman/logos/gnome-netstatus-error.png</logounavail>
    <status>
      <name>Internet reachable</name>
      <exec>/bin/ping</exec>
      <arg>-q</arg>
      <arg>-c</arg>
      <arg>1</arg>
      <arg>google.com</arg>
      <success>0</success>
      <logosuccess>./applets/netman/logos/network-up.png</logosuccess>
      <logofail>./applets/netman/logos/network-down.png</logofail>
    </status>
    <status>
      <name>Gateway reachable</name>
      <exec>/bin/ping</exec>
      <arg>-q</arg>
      <arg>-c</arg>
      <arg>1</arg>
      <arg>10.0.0.1</arg>
      <success>0</success>
      <logosuccess>./applets/netman/logos/network-up.png</logosuccess>
      <logofail>./applets/netman/logos/network-down.png</logofail>
    </status>
  </stati>
  <actions width="30%" height="10%" align="center">
    <action>
      <name>Restart network</name>
      <exec>./if-restart</exec>
      <arg>eth0</arg>
    </action>
  </actions>
</netman>
```

# TODO's #

  * Implement support for network unavailability when no network-interfaces have been configured.