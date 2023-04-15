// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAnalytics } from "firebase/analytics";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
// For Firebase JS SDK v7.20.0 and later, measurementId is optional
const firebaseConfig = {
  apiKey: "AIzaSyDOIDgSedybvSSPD2TejDmtdD0SZNoPS7U",
  authDomain: "aquiferia-f9786.firebaseapp.com",
  projectId: "aquiferia-f9786",
  storageBucket: "aquiferia-f9786.appspot.com",
  messagingSenderId: "236511073621",
  appId: "1:236511073621:web:2059e210c6ab5ac2e18bdc",
  measurementId: "G-52S8RPWSMV"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const analytics = getAnalytics(app);
console.log(app)