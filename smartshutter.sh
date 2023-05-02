#! /bin/bash
java -cp "UI/lib/groovy-4.0.10.jar;UI/lib/groovy-json-2.5.6.jar;UI/lib/groovy-swing-4.0.10.jar;UI/lib/jSerialComm-2.9.3.jar;." groovy.ui.GroovyMain UI/SmartShutter.groovy %1 %2
