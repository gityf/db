# #Basic information.
>This is client demo for database. MySQL Pgsql Oracle levelDB Redis memcached/CKV tcbdb gdbm.


# #pgsql
>To install pgsql server.
  * yum install postgresql-server
  * yum install postgresql-devel
>To initdb.
  * postgresql-setup initdb
>To start pgsql and create user with database.
  * systemctl restart postgresql
  * su - postgres
  * createuser dbuser
  * createdb -e -O dbuser dbname
>To set password for user 'dbuser'
  * su - postgres
  * psql
  * \password dbuser
