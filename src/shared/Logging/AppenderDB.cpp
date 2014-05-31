//
//  AppenderDB.cpp
//  TUMORS
//
//  Created by Jared Jones on 5/27/14.
//
//

#include "AppenderDB.h"

AppenderDB::AppenderDB(uint8 id, std::string const& name, LogLevel level)
: Appender(id, name, APPENDER_DB, level), realmId(0), enabled(false) { }

AppenderDB::~AppenderDB() { }

void AppenderDB::_write(LogMessage const& message)
{
    message.type.find("sql");//DELETE THIS, POINTLESS TO STOP WARNING
    
    /*
    // Avoid infinite loop, PExecute triggers Logging with "sql.sql" type
    if (!enabled || !message.type.find("sql"))
        return;
    
    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_INS_LOG);
    stmt->setUInt64(0, message.mtime);
    stmt->setUInt32(1, realmId);
    stmt->setString(2, message.type);
    stmt->setUInt8(3, uint8(message.level));
    stmt->setString(4, message.text);
    LoginDatabase.Execute(stmt);*/
    printf("WARNING: AppenderDB is not written yet due to lack of DB Support!!!");
}

void AppenderDB::setRealmId(uint32 _realmId)
{
    enabled = true;
    realmId = _realmId;
}