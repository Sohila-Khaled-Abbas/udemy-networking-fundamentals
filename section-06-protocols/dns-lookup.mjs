import {
  getServers,
  lookup,
  resolve4,
  resolve6,
  resolveMx,
} from 'node:dns/promises';

const hostname = process.argv[2] ?? 'example.com';

console.log(`DNS lab for: ${hostname}`);
console.log('Configured DNS servers:', getServers());
console.log('\n1) OS name lookup (may use hosts file or the OS resolver):');

try {
  const addresses = await lookup(hostname, { all: true, verbatim: true });
  console.table(addresses);
} catch (error) {
  console.error(`OS lookup failed: ${error.code ?? error.message}`);
}

async function showRecords(label, query) {
  console.log(`\n${label} records:`);
  try {
    const records = await query();
    console.dir(records, { depth: null });
  } catch (error) {
    console.log(`No result / query failed: ${error.code ?? error.message}`);
  }
}

await showRecords('A (IPv4)', () => resolve4(hostname, { ttl: true }));
await showRecords('AAAA (IPv6)', () => resolve6(hostname));
await showRecords('MX (mail exchangers)', () => resolveMx(hostname));

console.log('\nRemember: no AAAA or MX result can be normal for some names.');
