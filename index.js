 var tblUsers = document.getElementById('tbl_users_list');
  var databaseRef = firebase.database().ref('user/');
  var rowIndex = 1;
  
  databaseRef.once('value', function(snapshot) {
    snapshot.forEach(function(childSnapshot) {
   var childKey = childSnapshot.key;
   var childData = childSnapshot.val();
   
   var row = tblUsers.insertRow(rowIndex);
   var CO2_level=row.insertCell(0)
   var Space_avail = row.insertCell(1);
   var Space = row.insertCell(2);
  
   CO2_level.appendChild(document.createTextNode(childData.CO2_level));
  Space_avail.appendChild(document.createTextNode(childData.Space_avail));
   Space.appendChild(document.createTextNode(childData.Space));
   
   
   rowIndex = rowIndex + 1;

    });

  });
