## Basic informatiom.
>This is client demo for database. MySQL Pgsql Oracle levelDB Redis memcached/CKV tcbdb gdbm.
##
*pgsql
>To install pgsql server.
  yum install postgresql-server
  yum install postgresql-devel
To initdb.
  postgresql-setup initdb
To start pgsql and create user with database.
  <p>systemctl restart postgresql
  su - postgres
  createuser dbuser
  createdb -e -O dbuser dbname</p>
To set password for user 'dbuser'
  >>su - postgres
  >>psql
  >>\password dbuser
