#include <QtTest/QtTest>

#include <db.h>

class Dependencies: public QObject
{
  Q_OBJECT
  private slots:
    void initTestCase();
    void berkeleyDbVersion();
    void berkeleyDbCreate();
    void cleanupTestCase();
};

void Dependencies::initTestCase()
{
}

void Dependencies::berkeleyDbVersion()
{
  int requiredMajor = 4;
  # ifdef Q_OS_FREEBSD
    int requiredMinor = 3;
  # else
    int requiredMinor = 1;
  # endif
  QString msg;

  printf("Using Berkeley DB version: %s\n", DB_VERSION_STRING);
  if (DB_VERSION_MAJOR != requiredMajor || DB_VERSION_MINOR < requiredMinor)
  {
    msg = QString("Require at least version %1.%2").arg(requiredMajor).arg(requiredMinor);
    QFAIL(QTest::toString(msg));
  }
}

void Dependencies::berkeleyDbCreate()
{
  DB *dbp;
  QString msg;
  int rc;

  rc = db_create(&dbp, NULL, 0);
  if (rc != 0)
  {
    msg = QString("Error creating database handle: %1").arg(db_strerror(rc));
    QFAIL(QTest::toString(msg));
  }

  rc = dbp->open(dbp, NULL, "test.db", NULL, DB_BTREE, DB_CREATE, 0664);
  if (rc != 0)
  {
    msg = QString("Error opening database: %1").arg(db_strerror(rc));
    dbp->close(dbp, 0);
    QFAIL(QTest::toString(msg));
  }

  if (dbp != NULL)
  {
    rc = dbp->close(dbp, 0);
    if (rc != 0)
    {
      msg = QString("Error closing database: %1").arg(db_strerror(rc));
      QFAIL(QTest::toString(msg));
    }
  }
}

void Dependencies::cleanupTestCase()
{
  DB *dbpRemove;
  db_create(&dbpRemove, NULL, 0);
  dbpRemove->remove(dbpRemove, "test.db", NULL, 0);
}

QTEST_MAIN(Dependencies)
#include "Dependencies.moc"
