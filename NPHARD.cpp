#include<bits/stdc++.h>
using namespace std;
int mn(int req,int i,int c[],int s[]) {
    if (req <= 0)return 0;
    if (i < 0)return INT_MAX;
    int take = c[i] + mn(req - s[i], i - 1, c, s);
    int notake = mn(req, i - 1, c, s);
    return min(take, notake);
}
int main() {

return 0;
}