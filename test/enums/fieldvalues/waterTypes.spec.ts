import { expect } from 'chai';
import { WaterType } from 'libdivecomputerjs';

describe('WaterType', () => {
    it('Has all WaterType', () => {
        expect(WaterType.Fresh).equals('Fresh');
        expect(WaterType.Salt).equals('Salt');
    });
});
