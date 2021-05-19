// leetcode 191: count the 1 in binary

pub struct Solution {
}

impl Solution {
    pub fn new() -> Solution {
        Solution {
        }   
    }   
    pub fn hammingWeight (n: u32) -> i32 {
        let mut count = 0;
        let mut nn = n;
        while nn > 0 { 
            if nn % 2 == 1 { 
                count += 1;
            }   
            nn = nn / 2;
        }
//        println!("{}", count);
        return count;
    }   
}
