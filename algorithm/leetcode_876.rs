// leetcode 867 transpose matrix

pub struct Solution {
}

impl Solution {
    pub fn transpose(a: Vec<Vec<i32>>) -> Vec<Vec<i32>> {
        let mut ret = Vec::new();

        for j in 0..a[0].len() {
            let mut tmp = Vec::new();
            for i in 0..a.len() {
                tmp.push(a[i][j]);
            }
            ret.push(tmp);
        }

        return ret;
    }

    pub fn print(a: Vec<Vec<i32>>) {
        for i in 0..a.len() {
            for j in 0..a[0].len() {
                print!("{} ", a[i][j]);
            }
            println!();
        }
    }
}
