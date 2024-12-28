


const SIZE = 64;

function hash_poly(m, x, y) {
  const a = (m * 7) & 0xFF;
  const b = (x * 11) & 0xFF;
  const c = (y * 13) & 0xFF;
  const r = (a + b + c) & 0xFF;
  return r % SIZE;
}

function hash_xor_fast(m, x, y) {
  const a = (m << 0) & 0xFF;
  const b = (x << 1) & 0xFF;
  const c = (y << 2) & 0xFF;
  return (a ^ b ^ c) & (64 - 1);
}

function hash_xor(m, x, y) {
  return (m ^ (x << 2) ^ (y << 3)) % SIZE;
}

function hash(m, x, y) {
  // return hash_xor(m, x, y);
  // return hash_poly(m, x, y);
  return hash_xor_fast(m, x, y);
}


let tests = [
  [0, 8, 1],
  [1, 4, 1],
  [0, 25, 27],
  [1, 4, 5],
  [0, 9, 27],
  [2, 3, 3],
  [0, 14, 1],
  [2, 4, 7],
  [2, 3, 7],
  [0, 19, 1],
  [0, 10, 5],
  [0, 12, 5],
  [0, 14, 5],
  [0, 16, 5],
  [0, 18, 5],
  [0, 20, 5],
  [0, 4, 1],
  [0, 7, 1],
  [0, 12, 3],
  [0, 22, 3],
  [0, 20, 3],
];


function rng(size) {
  return Math.floor(Math.random() * size);
}

// tests = [];
// for (let k = 0; k < 3 * 7; k++) {
//   tests.push([rng(8), rng(32), rng(32)]);
// }


function main() {
  const results = {}
  let collisions = 0;

  tests.forEach(testAry => {
    const index = hash.apply(null, testAry);
    if (results[index])
      collisions++;
    if (!results[index])
      results[index] = [];
    results[index].push(testAry);
  })

  let longest = 1;
  Object.keys(results).forEach(key => {
    if (results[key].length > longest)
      longest = results[key].length;
  })

  console.log(`Tested ${tests.length} coordinates.`)
  console.log(`Collisions: ${collisions}, longest bucket: ${longest}`)
}

main();
