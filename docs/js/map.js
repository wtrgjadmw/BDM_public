//初期値
var map;
var marker = [];
var com_data = [];
var windows = [];
var currentInfoWindow = null;

function initMap() {
  map = new google.maps.Map(document.getElementById('map'), { //GoogleMapを呼び出す処理
    //GoogleMapの初期値を設定
    center: { lat: 38.6460251, lng: 140.3427782}, //中心点（緯度・経度）
    zoom: 2.5, //拡大率
    clickableIcons: false, //GoogleMapの標準のクリック機能をOFFにする。
    // GoogleMapのスタイルを変更
    styles: [
      {
            "stylers": [
              { "visibility": "simplified" }
            ]
          },
          {
            "featureType": "water",
            "elementType": "geometry.fill",
            "stylers": [
              { "hue": "#003bff" }
            ]
          },{
            "featureType": "road.highway",
            "stylers": [
              { "visibility": "off" }
            ]
          },{
            "featureType": "landscape",
            "stylers": [
              { "color": "#dbe8e5" }
            ]
          },{
        "featureType": "poi.park",
        "elementType": "labels.text",
            "stylers": [
              { "color": "#e6e6e6","visibility": "off" }
            ]
          },{
            "featureType": "water",
            "stylers": [
              { "color": "#8ecdf0" }
            ]
          },{
            "featureType": "transit.line",
            "stylers": [
              { "visibility": "off" }
            ]
          },{
        "featureType": "poi.business",
        "elementType": "labels.text",
        "stylers": [
          {
            "visibility": "off"
          }
        ]
      }
    ]
  });
  
  // 企業名の受け取り
  var query = location.search;
  var value = query.split('=');
  pre_index_num = decodeURIComponent(value[1]);
  index_num = pre_index_num.substr(0,1);
  and_num = decodeURIComponent(value[2]);
  console.log(index_num);  
  console.log(and_num);  
  //会社情報を読み込み
  var marketcap; 
  var endpoint;

    if (index_num == "1" && and_num != "1" && and_num != "2"){
      endpoint = "https://script.google.com/macros/s/AKfycbwOI9Fv6U_6ATbP-n9fHY2yoyzDSl7nOEvfTrbm2_Kd6fHkr8l08UaZgQolTJweFPou/exec";
    } else if (index_num == "1" && and_num == "1") {
      endpoint = "https://script.google.com/macros/s/AKfycbwv3A6J9xxs8eUt4Y153RVksBH2PE0t50OFhz7DueVgStIDj_3VL-RVMle7Q_a5-6GV/exec";
    } else if (index_num == "1" && and_num == "2") {
      endpoint = "https://script.google.com/macros/s/AKfycbwBWuuPMUlOqTuKfZ378c3GpjUV9GUxQ96G6rn4dFCPqP3hts3DBDZ7m08uT9rEHkLrjA/exec";
    }
    else if (index_num == "2" && and_num != "1" && and_num != "2") {
      endpoint = "https://script.google.com/macros/s/AKfycbxKECOo9uOcXE9y55KkLqJ0uEVONJI6J9ohUa-W_OnrukE7ArSZH28nbC3ovza3QWhU/exec";
    } else if (index_num == "2" && and_num == "1") {
      endpoint = "https://script.google.com/macros/s/AKfycbyOKntp9Hskgd1yl5Rk6iD8R3d120kUC1aHESxezr8NBYZ94zzUy1eWf_5i4SjtxZMt/exec";
    } else if (index_num == "2" && and_num == "2") {
      endpoint = "https://script.google.com/macros/s/AKfycbykmrlMgG8IwjzUw6owGNhjJ0ZMy2383y0teKad0a-Z6VmMFEJRjsip6kkqJl4q1pG1/exec";
    } 
    else if (index_num == "3" && and_num != "1" && and_num != "2"){
      endpoint = "https://script.google.com/macros/s/AKfycbwtE75zfk0TQXQjS2VTu9SJ6sk2u5YG3bpVqIxokynHv9NF4h6nDDqqCvj8z6Hm8B6Zkw/exec";
    } else if (index_num == "3" && and_num == "1") {
      endpoint = "https://script.google.com/macros/s/AKfycbz-HNhuNkH9IBjjjT-ReZTSfsfjhin3cXgTIP8dmriCghDB_1uN8HrU4AbUhg5G_DByPw/exec";
    } else if (index_num == "3" && and_num == "2") {
      endpoint = "https://script.google.com/macros/s/AKfycbzEsWOs_c6Pp3FSB8ogZBSKEyydQevsNi38z5yJVmZKkzaFWcuV9pkliMOQd96DJuQDuQ/exec";
    } 


    fetch(endpoint)
    .then(response => response.json())
    /* 成功時の処理 */
    .then(data => {
        marketcap = data;
        /* 会社の所在地を表示 */
        dataset = marketcap;
        console.log(dataset.length);
        for (var i = 0; i <= dataset.length - 1; i++) {
          com_data.push(
            {
              'name': dataset[i].name,
              'Industry':dataset[i].Industry,
              'lng': dataset[i].lng,
              'lat': dataset[i].lat,
              'index':dataset[i].index
            }
          );
        };
        console.log(com_data);
        for (var i = 0; i < dataset.length; i++) {
          //吹き出しの追加
          markerLatLng = { lng: com_data[i]['lng'], lat: com_data[i]['lat'] };
          if (com_data[i]['index'] == "1"){
            marker[i] = new google.maps.Marker({
              position: markerLatLng,
              map: map,
              icon: '../images/map_icon1.png' //オリジナルのアイコン名
            });
          } else if (com_data[i]['index'] == "2"){
            marker[i] = new google.maps.Marker({
              position: markerLatLng,
              map: map,
              icon: '../images/map_icon2.png' //オリジナルのアイコン名
            });
          } else {
            marker[i] = new google.maps.Marker({
              position: markerLatLng,
              map: map,
              icon: '../images/map_icon3.png' //オリジナルのアイコン名
          });
          }
          function markerEvent(i) {
            marker[i].addListener('mouseover', function () { // マーカーをクリックしたとき
              //開いているウィンドウがある場合は閉じる
              if (currentInfoWindow) {
                currentInfoWindow.close();
              }
              windows[i].open(map, marker[i]); // 吹き出しの表示
              currentInfoWindow = windows[i];
            });
            marker[i].addListener('click', function () {
              window.location.href = 'companies.html?name=' + encodeURIComponent(com_data[i]['name']);
            });
          }
          windows[i] = new google.maps.InfoWindow({ //吹き出しの中身
            content: '<p class="location">' + com_data[i]['Industry'] +'<br>'+ com_data[i]['name'] + '</p></div>'
          });
          markerEvent(i); // マーカーにクリックイベントを追加
        }
    })
    /* 失敗時にエラーを吐く */
    .catch(error => {
        console.error('通信に失敗しました', error);
    });
}