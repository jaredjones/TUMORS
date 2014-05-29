//
//  AppenderDB.h
//  TUMORS
//
//  Created by Jared Jones on 5/27/14.
//
//

#ifndef __TUMORS__AppenderDB__
#define __TUMORS__AppenderDB__

class AppenderDB: public Appender
{
public:
    AppenderDB(uint8 _id, std::string const& _name, LogLevel level);
    ~AppenderDB();
    
    void setRealmId(uint32 realmId);
    
private:
    uint32 realmId;
    bool enabled;
    void _write(LogMessage const& message);
};

#endif /* defined(__TUMORS__AppenderDB__) */
