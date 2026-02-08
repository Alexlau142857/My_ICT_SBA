# ICT_SBA

* Haven't decided the topic and use of the system

 ***TODO*** :

* Use database (SQL) to store account data

* Create Audit Log (log.txt)

* Define Role Hierarchy (Admin, Moderator, User)

* other...


 ---
   
 > 2026.02.03

 - Already wrote a login system demo

 ---

 > 2026.02.05

 - upgrate the insert function (improve runtime O(N^2) -> O(NlogN) from sort to binary search)

 - add function (admin account can list out all the account id and password)

 - add function (admin account can output all the account id and password to "account.txt" file

 - debug: when the password is invaild, input the password again the program will check the password is vaild or not again

 - Other debugging and improvement...

 ---

 > 2026.02.06

 - add function (admin can insert account from txt file)

 - change the data type of account array from array -> std::vector > std::map

 - add function (admin can delete the account)

 - Other debugging and improvement...

 ---
 
 > 2026.02.07 (early morning)

 - rewrite the structure of the whole code

 - mordulize many function

 - Other debugging and improvement...
   
 ---

 > 2026.02.07


 - Mask password input (display '*' when typing)

 - Implement Account Lockout Mechanism (prevent brute force)

 - Add "Change Password" & "Forgot Password" features
   
 - Add "Show/Hide Password" toggle
   
 - Other debugging and improvement...
   
 ---
