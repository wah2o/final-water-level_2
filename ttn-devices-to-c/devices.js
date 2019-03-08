/**
 * > const ttn = require('ttn')
 * > var app; ttn.application('YOUR_APP', 'YOUR_KEY').then(a=>app=a)
 * > var d; app.devices().then(de=>d=de)
 * > d
 */

var devices =[
  { appId: '12345678909876543',
    devId: 'hc_sr04_2',
    latitude: 0.3956° S,
    longitude: 36.9622° E,
    altitude: 7723,
    description: 'water level system',
    fCntUp: 0,
    fCntDown: 0,
    disableFCntCheck: true,
    uses32BitFCnt: true,
    activationConstraints: 'local',
    usedDevNoncesList: [],
    usedAppNoncesList: [],
    lastSeen: 0,
    appEui: '70B3D57ED0010386',
    devEui: '00D76350D629D861',
    appKey: '00000000000000000000000000000000',
    appSKey: '8833967FB940B18EDD461ADE38905F2A',
    nwkSKey: '7B2C6805B56D7B055FA31CF21F0326D2',
    devAddr: '26011EE8',
    attributes: {} }]


console.log(devices.map((d) => {
  var id = d.devId.match(/(\d+)/)[0];
  return `static uint32_t DEVADDR_${id} = 0x${d.devAddr};
static uint8_t NWKSKEY_${id}[] = { ${d.nwkSKey.split(/(..)/).filter(f=>!!f).map(f=>'0x'+f).join(', ')} };
static uint8_t APPSKEY_${id}[] = { ${d.appSKey.split(/(..)/).filter(f=>!!f).map(f=>'0x'+f).join(', ')} };`
  }).join('\n\n'));
