nss-curl
========

nss-curl remote User-Database
this try fetch user database from a remote host, it use **curl** for fetching.

# INSTALL

Compile it clone:
~~~bash
git clone https://github.com/Neurotec/nss-curl.git
make
cp libnss_curl.so.2 /lib/
~~~

Copy from **/etc/nss-curl.conf** to host **/etc** and edit it acording your host and scripts.

Edit **/etc/nsswitch.conf** and add service **curl**, to:

  * group
  * host
  
# TODO

  * Required SSL
  * Rest of services.
  
# WORKING

  * Trisquel 6 (AMD64)
  
## EXAMPLE PHP

**gethostbyname.php**

~~~php
switch($_GET["index"])
{
case "localhost":
 echo "127.0.0.1"; 
default:
 header( 'HTTP/1.1 400 BAD REQUEST' );
}
~~~


**getgrent.php**

~~~php
if($_GET["index"] == 1)
{
 echo "test:3000:x:user1,user2";
}
else
{
 header( 'HTTP/1.1 400 BAD REQUEST' );
}
~~~

**getgrname.php**

~~~php
if($_GET["index"] == "test")
{
 echo "test:3000:x:user1,user2";
}
else
{
 header( 'HTTP/1.1 400 BAD REQUEST' );
}
~~~
