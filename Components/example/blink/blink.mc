Project{name:'mStack Blink Example'}
Log{name:'blink.log'}

Section{name:'Blink'}
IntField{name:'Interval', min: 10, max: 1000, readonly:false, command: 100}
DoubleButton{nameLeft: 'Start',nameRight: 'Stop',commandLeft:101, commandRight: 102}
