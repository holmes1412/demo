// leetcode 27: eleminate the val in Vec

pub struct Solution {
}

impl Solution {
    pub fn remove_element(nums: &mut Vec<i32>, val: i32) -> i32 {
 //       for i in &mut nums {
//        let mut iter = nums.iter().filter(|&&x| x!=val);
 //       let mut cur = 0;
        let mut pos : usize = 0;
 //       for i in nums.iter() {
        for i in 0..nums.len() {
            if nums[i] != val {
                nums[pos] = nums[i];
                pos = pos + 1;
            }
        }
        let ret = pos as i32;
        return ret;
    }

    pub fn print(nums: &mut Vec<i32>, len: i32) {
        let l = len as usize;
        for i in 0..l {
            println!("{}", nums[i]);
        }
    }
}
