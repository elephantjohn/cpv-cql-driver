#!/usr/bin/env python3
# -*- coding: utf-8 -*-
from cassandra.cluster import Cluster
from cassandra.query import SimpleStatement, BatchStatement
from cassandra import ConsistencyLevel
import sys
import time

LOOP_COUNT=10000
SELECT_COUNT=100
ENABLED_COMPRESSION="--compress" in sys.argv
ENABLED_PREPARATION="--prepare" in sys.argv
DEFAULT_CONSISTENCY=ConsistencyLevel.LOCAL_ONE

if __name__ == "__main__":
	if ENABLED_COMPRESSION:
		print("compression enabled")
	if ENABLED_PREPARATION:
		print("preparation enabled")

	cluster = Cluster(["127.0.0.1"], port=9043, compression=ENABLED_COMPRESSION)
	session = cluster.connect()
	session.default_consistency_level=DEFAULT_CONSISTENCY
	session.execute("drop keyspace if exists benchmark_ks")
	session.execute("create keyspace benchmark_ks with replication = " +
		"{ 'class': 'SimpleStrategy', 'replication_factor': 1 }")
	session.execute("create table benchmark_ks.my_table (id int primary key, name text)")

	insertStatement = SimpleStatement(
		"insert into benchmark_ks.my_table (id, name) values (%s, %s)")
	insertBatch = BatchStatement()
	for i in range(SELECT_COUNT*2):
		insertBatch.add(insertStatement, (i, "name"))
	session.execute(insertBatch)

	start=time.time()
	selectStr = "select id, name from benchmark_ks.my_table "
	limitStr = "limit %d"%SELECT_COUNT
	if ENABLED_PREPARATION:
		selectStatement = session.prepare(selectStr + limitStr)
	else:
		selectStatement = SimpleStatement(selectStr + limitStr)
	for i in range(LOOP_COUNT):
		rows = session.execute(selectStatement)
		for row in rows:
			id_, name = row.id, row.name
			# print(id_, name)
	print("used seconds: %s"%(time.time()-start))

