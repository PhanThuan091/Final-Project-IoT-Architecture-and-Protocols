import { initializeApp } from "https://www.gstatic.com/firebasejs/10.4.0/firebase-app.js";
import { getAuth, signInWithEmailAndPassword, createUserWithEmailAndPassword  } 
from "https://www.gstatic.com/firebasejs/10.4.0/firebase-auth.js";
const firebaseConfig = {
  apiKey: "AIzaSyBdSEuAKrHD1zvej5U5S8qNmk1Rcy7rxOA",
  authDomain: "login-5707d.firebaseapp.com",
  databaseURL: "https://login-5707d-default-rtdb.firebaseio.com",
  projectId: "login-5707d",
  storageBucket: "login-5707d.appspot.com",
  messagingSenderId: "654767418735",
  appId: "1:654767418735:web:b531ff7fcab81f8a42cc9a",
  measurementId: "G-4LVH017F9Y"
};

const app = initializeApp(firebaseConfig);
const auth = getAuth(app);

document.getElementById("loginButton").addEventListener('click', function () {
  const loginEmail = document.getElementById("email").value;
  const loginPassword = document.getElementById("password").value;


  signInWithEmailAndPassword(auth, loginEmail, loginPassword)
    .then((userCredential) => {
      // Signed in 
      const user = userCredential.user;
      window.location.href = "index.html";
    })
    .catch((error) => {
      const errorCode = error.code;
      const errorMessage = error.message;
      alert("Sorry! " + errorMessage);
    });

});

document.getElementById("registerButton").addEventListener('click', function () {
  const registerEmail = document.getElementById("email").value;
  const registerPassword = document.getElementById("password").value;


  createUserWithEmailAndPassword(auth, registerEmail, registerPassword)
    .then((userCredential) => {
      // Signed up 
      const user = userCredential.user;
      alert("Registered Successfully");
    })
    .catch((error) => {
      const errorCode = error.code;
      const errorMessage = error.message;
      alert("Sorry! " + errorMessage);
    });

});
