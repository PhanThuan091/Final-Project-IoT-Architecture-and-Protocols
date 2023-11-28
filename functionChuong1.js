// khoi tao cac thong so cho quat
let control__air__on = document.querySelector("#control__air__on");
let img2 = document.querySelector("#Fan");
let control__air__off = document.querySelector("#control__air__off");
var isFanStateChangeFromUser = false;

document.addEventListener("DOMContentLoaded", function () {
  var btn_lamp_Coop1 = localStorage.getItem("btn_lamp_Coop1");
  var btn_fan_Coop1 = localStorage.getItem("btn_fan_Coop1");
  var fanAutoState = localStorage.getItem("fanAutoState");

  //Fan
  if (btn_fan_Coop1 == "true") {
    document.getElementById("Fan").src = "./image/fan_on.gif";
  } else {
    document.getElementById("Fan").src = "./image/fan_off.png";
  }

  control__air__on.addEventListener("click", () => {
    isFanStateChangeFromUser = true;
    img2.src = "./image/fan_on.gif";
    firebase.database().ref("chuong1").update({ Quat: "1" });
    localStorage.setItem("btn_fan_Coop1", "true");
  });
  control__air__off.addEventListener("click", () => {
    isFanStateChangeFromUser = true;
    img2.src = "./image/fan_off.png";
    firebase.database().ref("chuong1").update({ Quat: "0" });
    localStorage.setItem("btn_fan_Coop1", "false");
  });

  quatRef.on("value", function (snapshot) {
    var quatStatus = snapshot.val();
    if (!isFanStateChangeFromUser) {
      if (quatStatus == "1") {
        img2.src = "./image/fan_on.gif";
        localStorage.setItem("btn_fan_Coop1", "true");
      } else {
        img2.src = "./image/fan_off.png";
        localStorage.setItem("btn_fan_Coop1", "false");
      }
    }
    isFanStateChangeFromUser = false;
  });
  var airImage = document.getElementById("Fan");
  var airButton = document.getElementById("control__air__on");

  // Lắng nghe sự thay đổi của nhiệt độ từ Firebase Realtime Database
  dbRef.on("value", function (snapshot) {
    var nhietdoValue = snapshot.val();
    // Kiểm tra nhiệt độ
    if (nhietdoValue > 40) {
      // Thay đổi hình ảnh và tự động bật quạt
      airImage.src = "./image/fan_on.gif";
      firebase.database().ref("chuong1").update({ Quat: "1" });
      localStorage.setItem("fanAutoState", "true");
      document.getElementById("nd__content").style.color = "red";
      if (fanAutoState === "true") {
        localStorage.setItem("btn_fan_Coop1", "true");
      }
    } else {
      // Khôi phục hình ảnh ban đầu, không tự động bật quạt
      if (fanAutoState === "true") {
        airImage.src = "./image/fan_off.png";
        firebase.database().ref("chuong1").update({ Quat: "0" });
        localStorage.setItem("btn_fan_Coop1", "false");
        localStorage.setItem("fanAutoState", "false");
        document.getElementById("nd__content").style.color = "black";
      }
    }
  });

  //Đèn
  var slider = document.getElementById("myRange");
  var img1 = document.querySelector("#Light");
  var output = document.getElementById("demo");
  var sliderValue = localStorage.getItem("sliderValue");
  var isLightStateChangeFromUser = false;

  // Đặt giá trị ban đầu của slider
  slider.value = sliderValue;
  output.innerHTML = sliderValue;
  updateBulbImage(sliderValue);

  slider.oninput = function () {
    isLightStateChangeFromUser = true;
    var value = this.value;
    output.innerHTML = value;
    updateBulbImage(value);
    localStorage.setItem("sliderValue", value);
  };
  function updateBulbImage(value) {
    if (value <= 100 && value > 70) {
      img1.src = "./image/fullLight.png";
      firebase.database().ref("chuong1").update({ Den: "1" });
    } else if (value >= 20 && value <= 70) {
      img1.src = "./image/halfLight.png";
      firebase.database().ref("chuong1").update({ Den: "0.5" });
    } else {
      img1.src = "./image/turned-off.png";
      firebase.database().ref("chuong1").update({ Den: "0" });
    }
  }

  // Cảm nhận sự thay đổi giá trị trên firebase
  DenRef.on("value", function (snapshot) {
    var bulbStatus = snapshot.val();
    if (!isLightStateChangeFromUser) {
      var valuefirebase =
        bulbStatus == "1" ? 100 : bulbStatus == "0.5" ? 50 : 1;
      updateBulbImage(valuefirebase);
      slider.value = valuefirebase;
      output.innerHTML = valuefirebase;
      localStorage.setItem("sliderValue", valuefirebase);
    }
    isLightStateChangeFromUser = false;
  });

  // Nút điều khiển cho ăn
  let feedImage = document.querySelector("#Feeding1");

  function changeFeedImage() {
    feedImage.src = "./image/feeding.gif";
    firebase.database().ref("chuong1").update({ Cho_an: "1" });

    setTimeout(function () {
      feedImage.src = "./image/feed.jpg";
      firebase.database().ref("chuong1").update({ Cho_an: "0" });
    }, 10000);
  }

  let feedButton = document.getElementById("control__feed__on");
  feedButton.addEventListener("click", changeFeedImage);

  FeedRef.on("value", function (snapshot) {
    var feedStatus = snapshot.val();
    if (feedStatus == "1") {
      feedImage.src = "./image/feeding.gif";
    } else {
      feedImage.src = "./image/feed.jpg";
    }
  });
});

window.addEventListener("beforeunload", function () {
  // Trước khi rời khỏi trang, đặt giá trị trạng thái trên Firebase về 0
  firebase.database().ref("chuong1").update({ Cho_an: "0" });
});
