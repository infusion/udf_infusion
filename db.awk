
match($0, /^(double|char|longlong)[ *]+([a-z0-9_]+)\(/, m) {

   if ("double" == m[1])
     used[m[2]] = "real";
   else if ("longlong" == m[1])
     used[m[2]] = "integer";
   else if ("char" == m[1])
     used[m[2]] = "string";
}

match($0, /([a-z0-9_]+)_add/, m) {
   aggregate[m[1]] = 1;
}

END {
   for (x in used) {

      if (drop) {
         printf("DROP FUNCTION IF EXISTS %s;\n", x);
      } else if (x in aggregate) {
         printf("CREATE AGGREGATE FUNCTION %s RETURNS %s SONAME 'udf_infusion.so';\n", x, used[x]);
      } else {
         printf("CREATE FUNCTION %s RETURNS %s SONAME 'udf_infusion.so';\n", x, used[x]);
      }
   }
}
