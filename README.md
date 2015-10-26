##Basic information.
>This is client demo for database. MySQL Pgsql Oracle levelDB Redis memcached/CKV tcbdb gdbm.


##pgsql
>To install pgsql server.
  * yum install postgresql-server
  * yum install postgresql-devel
  * postgresql-setup initdb
  * systemctl restart postgresql
  * su - postgres
  * createuser dbuser
  * createdb -e -O dbuser dbname
  * psql
  * \password dbuser
